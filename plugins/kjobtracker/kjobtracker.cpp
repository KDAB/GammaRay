/*
  kjobtracker.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "kjobtracker.h"
#include "kjobmodel.h"

#include <QDebug>
#include <QSortFilterProxyModel>
#include <QtPlugin>

using namespace GammaRay;

KJobTracker::KJobTracker(Probe *probe, QObject *parent)
    : QObject(parent)
    , m_jobModel(new KJobModel(this))
{
    connect(probe, &Probe::objectCreated, m_jobModel, &KJobModel::objectAdded);
    connect(probe, &Probe::objectDestroyed, m_jobModel, &KJobModel::objectRemoved);

    auto proxy = new QSortFilterProxyModel(this);
    proxy->setSourceModel(m_jobModel);

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.KJobModel"), proxy);
}

KJobTracker::~KJobTracker() = default;
