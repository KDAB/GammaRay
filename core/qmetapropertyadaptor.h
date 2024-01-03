/*
  qmetapropertyadaptor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QMETAPROPERTYADAPTOR_H
#define GAMMARAY_QMETAPROPERTYADAPTOR_H

#include "propertyadaptor.h"
#include "objectinstance.h"

namespace GammaRay {
/** Property adaptor for QMetaProperty/Object-based property access. */
class QMetaPropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit QMetaPropertyAdaptor(QObject *parent = nullptr);
    ~QMetaPropertyAdaptor() override;

    int count() const override;
    PropertyData propertyData(int index) const override;
    void writeProperty(int index, const QVariant &value) override;
    void resetProperty(int index) override;

protected:
    void doSetObject(const ObjectInstance &oi) override;

private:
    QString detailString(const QMetaProperty &prop) const;
    PropertyData propertyMetaData(int index) const;

private slots:
    void propertyUpdated();

private:
    QHash<int, int> m_notifyToRowMap;
    QVector<int> m_rowToPropertyIndex;
    mutable bool m_notifyGuard;
};
}

#endif // GAMMARAY_QMETAPROPERTYADAPTOR_H
