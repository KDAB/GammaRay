/*
  quickinspectorinterface.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickinspectorinterface.h"
#include "transferimage.h"

#include <common/objectbroker.h>

#include <QDataStream>

using namespace GammaRay;

QDataStream &operator<<(QDataStream &out, QuickInspectorInterface::Features value)
{
  out << qint32(value);
  return out;
}

QDataStream &operator>>(QDataStream &in, QuickInspectorInterface::Features &value)
{
  qint32 t;
  in >> t;
  value = static_cast<QuickInspectorInterface::Features>(t);
  return in;
}

QDataStream &operator<<(QDataStream &out, QuickInspectorInterface::RenderMode value)
{
  out << qint32(value);
  return out;
}

QDataStream &operator>>(QDataStream &in, QuickInspectorInterface::RenderMode &value)
{
  qint32 t;
  in >> t;
  value = static_cast<QuickInspectorInterface::RenderMode>(t);
  return in;
}

QuickInspectorInterface::QuickInspectorInterface(QObject * parent) : QObject(parent)
{
  ObjectBroker::registerObject<QuickInspectorInterface*>(this);
  qRegisterMetaTypeStreamOperators<Features>();
  qRegisterMetaTypeStreamOperators<RenderMode>();
  qRegisterMetaTypeStreamOperators<TransferImage>();
}

QuickInspectorInterface::~QuickInspectorInterface()
{
}
