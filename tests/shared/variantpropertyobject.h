/*
  variantpropertyobject.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
