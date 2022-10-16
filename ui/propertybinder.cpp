/*
  propertybinder.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "propertybinder.h"

#include <compat/qasconst.h>

#include <QDebug>
#include <QMetaProperty>

using namespace GammaRay;

PropertyBinder::PropertyBinder(QObject *source, QObject *destination)
    : QObject(source)
    , m_source(source)
    , m_destination(destination)
    , m_lock(false)
{
    Q_ASSERT(source);
    Q_ASSERT(destination);
}

PropertyBinder::PropertyBinder(QObject *source, const char *sourceProp, QObject *destination,
                               const char *destProp)
    : QObject(source)
    , m_source(source)
    , m_destination(destination)
    , m_lock(false)
{
    Q_ASSERT(source);
    Q_ASSERT(destination);

    add(sourceProp, destProp);
    syncSourceToDestination();
}

PropertyBinder::~PropertyBinder() = default;

void PropertyBinder::add(const char *sourceProp, const char *destProp)
{
    Q_ASSERT(sourceProp);
    Q_ASSERT(destProp);

    Binding b;
    const auto sourceIndex = m_source->metaObject()->indexOfProperty(sourceProp);
    b.sourceProperty = m_source->metaObject()->property(sourceIndex);
    Q_ASSERT(b.sourceProperty.isValid());
    Q_ASSERT(b.sourceProperty.hasNotifySignal());
    connect(m_source, QByteArray("2") + b.sourceProperty.notifySignal().methodSignature(), this, SLOT(syncSourceToDestination()));

    const auto destIndex = m_destination->metaObject()->indexOfProperty(destProp);
    b.destinationProperty = m_destination->metaObject()->property(destIndex);
    Q_ASSERT(b.destinationProperty.isValid());
    Q_ASSERT(b.destinationProperty.isWritable());

    m_properties.push_back(b);

    // notification for reverse direction changes, if present
    if (!b.destinationProperty.hasNotifySignal() || !b.sourceProperty.isWritable())
        return;

    connect(m_destination, QByteArray("2") + b.destinationProperty.notifySignal().methodSignature(), this, SLOT(syncDestinationToSource()));
}

void PropertyBinder::syncSourceToDestination()
{
    if (!m_destination || m_lock)
        return;

    m_lock = true;
    for (const auto &b : qAsConst(m_properties))
        b.destinationProperty.write(m_destination, b.sourceProperty.read(m_source));
    m_lock = false;
}

void PropertyBinder::syncDestinationToSource()
{
    if (m_lock)
        return;

    m_lock = true;
    for (const auto &b : qAsConst(m_properties)) {
        if (!b.sourceProperty.isWritable())
            continue;
        b.sourceProperty.write(m_source, b.destinationProperty.read(m_destination));
    }
    m_lock = false;
}
