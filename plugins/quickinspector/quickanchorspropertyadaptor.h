/*
  quickanchorspropertyadaptor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKANCHORSPROPERTYADAPTOR_H
#define GAMMARAY_QUICKANCHORSPROPERTYADAPTOR_H

#include <core/propertyadaptor.h>
#include <core/propertyadaptorfactory.h>

#include <QVector>

namespace GammaRay {
class QuickAnchorsPropertyAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit QuickAnchorsPropertyAdaptor(QObject *parent = nullptr);
    ~QuickAnchorsPropertyAdaptor() override;

    int count() const override;
    PropertyData propertyData(int index) const override;

protected:
    void doSetObject(const ObjectInstance &oi) override;

private:
    int m_anchorsPropertyIndex;
    mutable bool m_notifyGuard;
};

class QuickAnchorsPropertyAdaptorFactory : public AbstractPropertyAdaptorFactory
{
public:
    PropertyAdaptor *create(const ObjectInstance &oi, QObject *parent = nullptr) const override;
    static QuickAnchorsPropertyAdaptorFactory *instance();

private:
    static QuickAnchorsPropertyAdaptorFactory *s_instance;
};
}

#endif
