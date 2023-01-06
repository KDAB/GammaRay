/*
  jsonpropertyadaptor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Nicolas Fella <nicolas.fella@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_JSONPROPERTYADAPTOR_H
#define GAMMARAY_JSONPROPERTYADAPTOR_H

#include "propertyadaptor.h"

#include <QJsonObject>
#include <QJsonArray>

namespace GammaRay {
/** Adaptor for recursing into jsonobject container property values. */
class JsonPropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit JsonPropertyAdaptor(QObject *parent = nullptr);
    ~JsonPropertyAdaptor() override;

    int count() const override;
    PropertyData propertyData(int index) const override;

protected:
    void doSetObject(const ObjectInstance &oi) override;

private:
    QJsonObject m_object;
    QJsonArray m_array;
    bool m_isObject;
};
}

#endif // GAMMARAY_JSONPROPERTYADAPTOR_H
