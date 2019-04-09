/*
  variantinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#ifndef VARIANTPROPERTYOBJECT_H
#define VARIANTPROPERTYOBJECT_H

#include <QPointer>
#include <QObject>
#include <QVector>
#include <QSharedPointer>

class VariantPropertyObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSharedPointer<QObject> sharedObject READ sharedObject CONSTANT)
    Q_PROPERTY(QPointer<QObject> trackingObject READ trackingObject CONSTANT)
    Q_PROPERTY(QVector<int> intVector READ widgetVector CONSTANT)
public:
    explicit VariantPropertyObject(QObject *parent = nullptr);
    ~VariantPropertyObject() override;

    QSharedPointer<QObject> sharedObject() const;
    QPointer<QObject> trackingObject() const;
    QVector<int> widgetVector() const;

private:
    QSharedPointer<QObject> m_object;
};

#endif
