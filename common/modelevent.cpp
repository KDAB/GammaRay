/*
  modelevent.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "modelevent.h"

#include <QAbstractItemModel>
#include <QCoreApplication>

using namespace GammaRay;

ModelEvent::ModelEvent(bool modelUsed)
    : QEvent(eventType())
    , m_used(modelUsed)
{
}

ModelEvent::~ModelEvent() = default;

bool ModelEvent::used() const
{
    return m_used;
}

QEvent::Type ModelEvent::eventType()
{
    static int id = -1;
    if (id < 0)
        id = registerEventType();
    return static_cast<QEvent::Type>(id);
}

void Model::used(const QAbstractItemModel *model)
{
    Q_ASSERT(model);
    ModelEvent ev(true);
    QCoreApplication::sendEvent(const_cast<QAbstractItemModel *>(model), &ev); // const_cast hack is needed since QItemSelectionModel gives us const*...
}

void Model::unused(QAbstractItemModel *model)
{
    Q_ASSERT(model);
    ModelEvent ev(false);
    QCoreApplication::sendEvent(model, &ev);
}
