/*
  propertybinder.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "propertybinder.h"

#include <QDebug>
#include <QMetaProperty>

using namespace GammaRay;

PropertyBinder::PropertyBinder(QObject* source, const char* sourceProp, QObject* destination, const char* destProp):
    QObject(source),
    m_source(source),
    m_destination(destination),
    m_lock(false)
{
    Q_ASSERT(source);
    Q_ASSERT(sourceProp);
    Q_ASSERT(destination);
    Q_ASSERT(destProp);

    const auto sourceIndex = source->metaObject()->indexOfProperty(sourceProp);
    m_sourceProperty = source->metaObject()->property(sourceIndex);
    Q_ASSERT(m_sourceProperty.isValid());
    Q_ASSERT(m_sourceProperty.hasNotifySignal());
    connect(source, QByteArray("2") +
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        m_sourceProperty.notifySignal().signature()
#else
        m_sourceProperty.notifySignal().methodSignature()
#endif
        , this, SLOT(sourceChanged()));

    const auto destIndex = destination->metaObject()->indexOfProperty(destProp);
    m_destinationProperty = destination->metaObject()->property(destIndex);
    Q_ASSERT(m_destinationProperty.isValid());
    Q_ASSERT(m_destinationProperty.isWritable());

    // initial sync
    const auto value = m_sourceProperty.read(source);
    const auto result = m_destinationProperty.write(destination, value);
    Q_ASSERT(result);

    // notification for reverse direction changes
    if (!m_destinationProperty.hasNotifySignal() || !m_sourceProperty.isWritable())
        return;

    connect(destination, QByteArray("2") +
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        m_destinationProperty.notifySignal().signature()
#else
        m_destinationProperty.notifySignal().methodSignature()
#endif
        , this, SLOT(destinationChanged()));
}

PropertyBinder::~PropertyBinder()
{
}

void PropertyBinder::sourceChanged()
{
    if (!m_destination || m_lock)
        return;

    const auto value = m_sourceProperty.read(m_source);
    m_lock = true;
    const auto result = m_destinationProperty.write(m_destination, value);
    Q_ASSERT(result);
    m_lock = false;
}

void PropertyBinder::destinationChanged()
{
    if (m_lock)
      return;

    const auto value = m_destinationProperty.read(m_destination);
    m_lock = true;
    const auto result = m_sourceProperty.write(m_source, value);
    Q_ASSERT(result);
    m_lock = false;
}
