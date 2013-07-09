/*
  signalmonitorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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

#include <core/tools/signalmonitor/signalhistorymodel.h>
#include <common/objectbroker.h>
#include <ui/deferredresizemodesetter.h>
#include <kde/krecursivefilterproxymodel.h>

#include <QHelpEvent>
#include <QToolTip>

#include <cmath>

using namespace GammaRay;

const QString SignalMonitorWidget::ITEM_TYPE_NAME_OBJECT = "Object";

SignalMonitorWidget::SignalMonitorWidget(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::SignalMonitorWidget)
  , m_eventDelegate(new SignalHistoryDelegate(this))
{
  ui->setupUi(this);

  QAbstractItemModel *const signalHistory = ObjectBroker::model("com.kdab.GammaRay.SignalHistoryModel");
  QSortFilterProxyModel *const objectProxy = new QSortFilterProxyModel(this);
  objectProxy->setSourceModel(signalHistory);
  objectProxy->setFilterKeyColumn(SignalHistoryModel::EventColumn);
  objectProxy->setFilterRole(SignalHistoryModel::ItemTypeNameRole);
  objectProxy->setFilterFixedString(ITEM_TYPE_NAME_OBJECT);

  QSortFilterProxyModel *const searchProxy = new KRecursiveFilterProxyModel(this);
  searchProxy->setSourceModel(objectProxy);
  searchProxy->setDynamicSortFilter(true);
  ui->objectSearchLine->setProxy(searchProxy);

  ui->objectTreeView->setModel(searchProxy);
  new DeferredResizeModeSetter(ui->objectTreeView->header(), 0, QHeaderView::Interactive);
  new DeferredResizeModeSetter(ui->objectTreeView->header(), 1, QHeaderView::Interactive);
  new DeferredResizeModeSetter(ui->objectTreeView->header(), 2, QHeaderView::Stretch);

  //ui->objectTreeView->setSelectionModel(ObjectBroker::selectionModel(ui->objectTreeView->model()));
  ui->objectTreeView->setItemDelegateForColumn(SignalHistoryModel::EventColumn, m_eventDelegate);

  connect(m_eventDelegate, SIGNAL(visibleIntervalChanged(qint64)), this, SLOT(adjustEventRange()));
  connect(m_eventDelegate, SIGNAL(visibleOffsetChanged(qint64)), this, SLOT(adjustEventRange()));
  connect(m_eventDelegate, SIGNAL(isActiveChanged(bool)), this, SLOT(eventsActiveChanged(bool)));
  connect(ui->intervalScale, SIGNAL(valueChanged(int)), this, SLOT(adjustEventScale(int)));
  connect(ui->pauseButton, SIGNAL(toggled(bool)), this, SLOT(pauseAndResume(bool)));
  connect(ui->objectTreeView->header(), SIGNAL(sectionResized(int, int, int)), this, SLOT(adjustEventScrollBarSize()));
  connect(ui->eventScrollBar, SIGNAL(valueChanged(int)), this, SLOT(adjustEventOffset(int)));
  connect(ui->eventScrollBar, SIGNAL(sliderMoved(int)), this, SLOT(adjustEventOffset(int)));
}

SignalMonitorWidget::~SignalMonitorWidget()
{
}

void SignalMonitorWidget::adjustEventScale(int value)
{
  // FIXME: define more reasonable formula
  qint64 i = std::pow(1.07, value) * 5000;
  m_eventDelegate->setVisibleInterval(i);
}

void SignalMonitorWidget::adjustEventRange()
{
  ui->objectTreeView->viewport()->update(eventColumnPosition(), 0,
                                         eventColumnWidth(), ui->objectTreeView->height());

  const bool signalsBlocked = ui->eventScrollBar->blockSignals(true);

  // With 31 bits we cover more than 24 days when counting milliseconds.
  // That's much more time than this tool can handle. IMHO.
  ui->eventScrollBar->setMaximum(qMax(m_eventDelegate->totalInterval() -
                                      m_eventDelegate->visibleInterval(),
                                      0LL));

  ui->eventScrollBar->setSingleStep(m_eventDelegate->visibleInterval() / 10);
  ui->eventScrollBar->setPageStep(m_eventDelegate->visibleInterval());

  if (m_eventDelegate->isActive()) {
    ui->eventScrollBar->setValue(ui->eventScrollBar->maximum());
  }

  ui->eventScrollBar->blockSignals(signalsBlocked);
}

void SignalMonitorWidget::adjustEventOffset(int value)
{
  m_eventDelegate->setActive(false);
  m_eventDelegate->setVisibleOffset(value);
}

void SignalMonitorWidget::adjustEventScrollBarSize()
{
  const QWidget *const scrollBar = ui->objectTreeView->verticalScrollBar();
  const QWidget *const viewport = ui->objectTreeView->viewport();

  const int eventColumnLeft = eventColumnPosition();
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
  m_eventDelegate->setActive(not pause);
}

void SignalMonitorWidget::eventsActiveChanged(bool active)
{
  ui->pauseButton->setChecked(not active);
}

int SignalMonitorWidget::eventColumnPosition() const
{
  return ui->objectTreeView->columnViewportPosition(SignalHistoryModel::EventColumn);
}

int SignalMonitorWidget::eventColumnWidth() const
{
  return ui->objectTreeView->columnWidth(SignalHistoryModel::EventColumn);
}

bool SignalMonitorWidget::event(QEvent *event)
{
  if (event->type() == QEvent::ToolTip) {
    const QHelpEvent *const help = static_cast<QHelpEvent *>(event);
    if (childAt(help->x(), help->y()) == ui->objectTreeView->viewport()) {
      const QPoint &pos = ui->objectTreeView->viewport()->mapFrom(this, help->pos());
      const QModelIndex index = ui->objectTreeView->indexAt(pos);

      if (index.isValid() && index.column() == SignalHistoryModel::EventColumn) {
        const int x0 = pos.x() - eventColumnPosition();
        const int dx = eventColumnWidth();
        const QString &toolTipText = m_eventDelegate->toolTipAt(index, x0, dx);

        if (not toolTipText.isEmpty()) {
          QToolTip::showText(help->globalPos(), toolTipText);
        } else {
          QToolTip::hideText();
          event->ignore();
        }

        return true;
      }
    }
  }

  return QWidget::event(event);
}

