/*
  kjobtracker.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "kjobtracker.h"
#include "kjobmodel.h"
#include "ui_kjobtracker.h"

using namespace GammaRay;

#include "include/probeinterface.h"

#include <QDebug>
#include <QtPlugin>
#include <QSortFilterProxyModel>

using namespace GammaRay;

KJobModel *KJobTracker::m_jobModel = 0;

KJobTracker::KJobTracker(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent), ui(new Ui::KJobTracker)
{
  Q_UNUSED(probe);
  ui->setupUi(this);

  QSortFilterProxyModel *filter = new QSortFilterProxyModel(this);
  filter->setSourceModel(m_jobModel);
  ui->searchLine->setProxy(filter);
  ui->jobView->setModel(filter);
}

KJobTracker::~KJobTracker()
{
}

void KJobTrackerFactory::init(ProbeInterface *probe)
{
  GammaRay::StandardToolFactory<KJob, GammaRay::KJobTracker>::init(probe);

  if (!KJobTracker::m_jobModel) {
    KJobTracker::m_jobModel = new KJobModel(this);

    connect(probe->probe(), SIGNAL(objectCreated(QObject*)),
            KJobTracker::m_jobModel, SLOT(objectAdded(QObject*)));
    connect(probe->probe(), SIGNAL(objectDestroyed(QObject*)),
            KJobTracker::m_jobModel, SLOT(objectRemoved(QObject*)));
  }
}

Q_EXPORT_PLUGIN(KJobTrackerFactory)

#include "kjobtracker.moc"
