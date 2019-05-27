/*
  qmlattachedpropertyadaptor.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_QMLATTACHEDPROPERTYADAPTOR_H
#define GAMMARAY_QMLATTACHEDPROPERTYADAPTOR_H

#include <core/propertyadaptor.h>
#include <core/propertyadaptorfactory.h>

#include <qqmlprivate.h>

namespace GammaRay {
class QmlAttachedPropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit QmlAttachedPropertyAdaptor(QObject *parent = nullptr);
    ~QmlAttachedPropertyAdaptor() override;

    int count() const override;
    PropertyData propertyData(int index) const override;

protected:
    void doSetObject(const ObjectInstance &oi) override;

private:
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 4)
    QVector<QQmlAttachedPropertiesFunc> m_attachedTypes;
#else
    QVector<int> m_attachedTypes;
#endif
};

/** QML attached property adaptor. */
class QmlAttachedPropertyAdaptorFactory : public AbstractPropertyAdaptorFactory
{
public:
    PropertyAdaptor *create(const ObjectInstance &oi, QObject *parent = nullptr) const override;
    static QmlAttachedPropertyAdaptorFactory *instance();
private:
    static QmlAttachedPropertyAdaptorFactory *s_instance;
};
}

#endif // GAMMARAY_QMLATTACHEDPROPERTYADAPTOR_H
