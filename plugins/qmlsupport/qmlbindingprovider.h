/*
  qmlbindingprovider.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QMLBINDINGPROVIDER_H
#define GAMMARAY_QMLBINDINGPROVIDER_H

// Own
#include <common/sourcelocation.h>
#include <core/abstractbindingprovider.h>

// Qt
#include <QQmlProperty>

// Std
#include <memory>
#include <vector>

QT_BEGIN_NAMESPACE
class QQmlAbstractBinding;
class QQmlBinding;
QT_END_NAMESPACE

namespace GammaRay {

class QmlBindingProvider : public AbstractBindingProvider
{
public:
    std::vector<std::unique_ptr<BindingNode>> findBindingsFor(QObject *obj) const override;
    std::vector<std::unique_ptr<BindingNode>> findDependenciesFor(GammaRay::BindingNode *binding) const override;
    bool canProvideBindingsFor(QObject *object) const override;

private:
    std::unique_ptr<BindingNode> bindingNodeFromQmlProperty(QQmlProperty property, BindingNode *parent) const;
    BindingNode *bindingNodeFromBinding(QQmlAbstractBinding *binding) const;
    static void fetchSourceLocationFor(BindingNode *node, QQmlBinding *binding);
    static QQmlAbstractBinding *bindingForProperty(QObject *obj, int propertyIndex);
};

}

#endif // GAMMARAY_QMLBINDINGPROVIDER_H
