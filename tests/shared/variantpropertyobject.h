/*
  variantpropertyobject.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    static QVector<int> widgetVector();

private:
    QSharedPointer<QObject> m_object;
};

#endif
