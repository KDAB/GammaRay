/*
  positioninfopropertyadaptor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_POSITIONINFOPROPERTYADAPTOR_H
#define GAMMARAY_POSITIONINFOPROPERTYADAPTOR_H

#include <core/propertyadaptor.h>
#include <core/propertyadaptorfactory.h>

namespace GammaRay {
class PositionInfoPropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit PositionInfoPropertyAdaptor(QObject *parent = nullptr);
    ~PositionInfoPropertyAdaptor() override;

    int count() const override;
    PropertyData propertyData(int index) const override;
};

class PositionInfoPropertyAdaptorFactory : public AbstractPropertyAdaptorFactory
{
public:
    PropertyAdaptor *create(const ObjectInstance &oi, QObject *parent = nullptr) const override;
    static PositionInfoPropertyAdaptorFactory *instance();

private:
    static PositionInfoPropertyAdaptorFactory *s_instance;
};

}

#endif // GAMMARAY_POSITIONINFOPROPERTYADAPTOR_H
