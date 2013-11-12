/*
  multisignalmapper.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "multisignalmapper.h"

#include <QDebug>
#include <QMetaEnum>
#include <QMetaObject>
#include <QSignalMapper>

using namespace GammaRay;

MultiSignalMapper::MultiSignalMapper(QObject *parent) : QObject(parent)
{
}

MultiSignalMapper::~MultiSignalMapper()
{
  qDeleteAll(m_mappers);
  m_mappers.clear();
}

void MultiSignalMapper::connectToSignal(QObject *sender, const QMetaMethod &signal)
{
  if (m_mappers.size() <= signal.methodIndex()) {
    m_mappers.resize(signal.methodIndex() + 1);
  }

  QSignalMapper *mapper = m_mappers.at(signal.methodIndex());
  if (!mapper) {
    mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(QObject*)), SLOT(slotMapped(QObject*)));
    m_mappers[signal.methodIndex()] = mapper;
  }

  mapper->setMapping(sender, sender);
  connect(sender, QByteArray::number(QSIGNAL_CODE) +
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
          signal.signature(),
#else
          signal.methodSignature(),
#endif
          mapper, SLOT(map()), Qt::UniqueConnection);
}

void MultiSignalMapper::slotMapped(QObject *object)
{
  emit signalEmitted(object, m_mappers.indexOf(static_cast<QSignalMapper*>(sender())));
}

