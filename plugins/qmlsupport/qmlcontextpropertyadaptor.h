/*
  qmlcontextpropertyadaptor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QMLCONTEXTPROPERTYADAPTOR_H
#define GAMMARAY_QMLCONTEXTPROPERTYADAPTOR_H

#include <core/propertyadaptor.h>
#include <core/propertyadaptorfactory.h>

#include <QVector>

namespace GammaRay {
class QmlContextPropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit QmlContextPropertyAdaptor(QObject *parent = nullptr);
    ~QmlContextPropertyAdaptor() override;

    int count() const override;
    PropertyData propertyData(int index) const override;
    void writeProperty(int index, const QVariant &value) override;

protected:
    void doSetObject(const ObjectInstance &oi) override;

private:
    QVector<QString> m_contextPropertyNames;
};

class QmlContextPropertyAdaptorFactory : public AbstractPropertyAdaptorFactory
{
public:
    PropertyAdaptor *create(const ObjectInstance &oi, QObject *parent = nullptr) const override;
    static QmlContextPropertyAdaptorFactory *instance();

private:
    static QmlContextPropertyAdaptorFactory *s_instance;
};
}

#endif
