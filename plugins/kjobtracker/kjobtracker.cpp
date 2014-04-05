/*
  kjobtracker.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <core/probeinterface.h>

#include <QDebug>
#include <QtPlugin>

using namespace GammaRay;

KJobTracker::KJobTracker(ProbeInterface *probe, QObject *parent)
  : QObject(parent), m_jobModel(new KJobModel(this))
{
  connect(probe->probe(), SIGNAL(objectCreated(QObject*)),
          m_jobModel, SLOT(objectAdded(QObject*)));
  connect(probe->probe(), SIGNAL(objectDestroyed(QObject*)),
          m_jobModel, SLOT(objectRemoved(QObject*)));

  probe->registerModel("com.kdab.GammaRay.KJobModel", m_jobModel);
}

KJobTracker::~KJobTracker()
{
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(KJobTrackerFactory)
#endif
