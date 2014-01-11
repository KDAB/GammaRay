/*
  methodargument.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "methodargument.h"

#include <QSharedData>
#include <QMetaType>

using namespace GammaRay;

class GammaRay::MethodArgumentPrivate : public QSharedData
{
  public:
    MethodArgumentPrivate() : QSharedData(), data(0) {}

    MethodArgumentPrivate(const MethodArgumentPrivate &other) : QSharedData(other)
    {
      value = other.value;
      name = other.name;
      data = 0;
    }

    ~MethodArgumentPrivate()
    {
      if (data)
        QMetaType::destroy(value.type(), data);
    }

    QVariant value;
    QByteArray name;
    void *data;
};

MethodArgument::MethodArgument() : d(new MethodArgumentPrivate)
{
}

MethodArgument::MethodArgument(const QVariant& v) : d(new MethodArgumentPrivate)
{
  d->value = v;
  d->name = v.typeName();
}

MethodArgument::MethodArgument(const MethodArgument& other) : d(other.d)
{
}

MethodArgument::~MethodArgument()
{
}

MethodArgument& MethodArgument::operator=(const MethodArgument& other)
{
  d = other.d;
  return *this;
}

MethodArgument::operator QGenericArgument() const
{
  if (d->value.isValid()) {
    d->data = QMetaType::construct(d->value.userType(), d->value.constData());
    Q_ASSERT(d->data);
    return QGenericArgument(d->name.data(), d->data);
  }
  return QGenericArgument();
}
