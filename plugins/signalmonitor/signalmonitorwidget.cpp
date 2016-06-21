/*
  signalmonitorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>

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
    new SearchLineController(ui->objectSearchLine, signalHistory);

    ui->objectTreeView->header()->setObjectName("objectTreeViewHeader");
    ui->objectTreeView->setModel(signalHistory);
    ui->objectTreeView->setEventScrollBar(ui->eventScrollBar);

    connect(ui->pauseButton, SIGNAL(toggled(bool)), this, SLOT(pauseAndResume(bool)));
    connect(ui->intervalScale, SIGNAL(valueChanged(int)), this,
            SLOT(intervalScaleValueChanged(int)));
    connect(ui->objectTreeView->eventDelegate(), SIGNAL(isActiveChanged(bool)), this,
            SLOT(eventDelegateIsActiveChanged(bool)));
    connect(ui->objectTreeView->header(), SIGNAL(sectionResized(int,int,int)), this,
            SLOT(adjustEventScrollBarSize()));

    m_stateManager.setDefaultSizes(ui->objectTreeView->header(),
                                   UISizeVector() << 200 << 200 << -1);
}

SignalMonitorWidget::~SignalMonitorWidget()
{
}

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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(SignalMonitorUiFactory)
#endif
