/*
  quickimplicitbindingdependencyprovider.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKIMPLICITBINDINGDEPENDENCYPROVIDER_H
#define GAMMARAY_QUICKIMPLICITBINDINGDEPENDENCYPROVIDER_H

// Own
#include <common/sourcelocation.h>
#include <core/abstractbindingprovider.h>

// Qt
#include <QQmlProperty>

// Std
#include <memory>
#include <vector>

QT_BEGIN_NAMESPACE
class QQuickItem;
class QQuickAnchors;
QT_END_NAMESPACE

namespace GammaRay {

class QuickImplicitBindingDependencyProvider : public AbstractBindingProvider
{
public:
    std::vector<std::unique_ptr<BindingNode>> findBindingsFor(QObject *obj) const override;
    std::vector<std::unique_ptr<BindingNode>> findDependenciesFor(GammaRay::BindingNode *binding) const override;
    bool canProvideBindingsFor(QObject *object) const override;

private:
    static std::unique_ptr<BindingNode> createBindingNode(QObject *obj, const char *propertyName, BindingNode *parent = nullptr);
    void anchorBindings(std::vector<std::unique_ptr<BindingNode>> &dependencies, QQuickAnchors *anchors, int propertyIndex, BindingNode *parent = nullptr) const;
    template<class Func>
    void childrenRectDependencies(QQuickItem *item, Func addDependency) const;
    template<class Func>
    void positionerDependencies(QQuickItem *item, Func addDependency) const;
    template<class Func>
    void implicitSizeDependencies(QQuickItem *item, Func addDependency) const;
    template<class Func>
    void anchoringDependencies(QQuickItem *item, Func addDependency) const;
};

}

#endif // GAMMARAY_QUICKIMPLICITBINDINGDEPENDENCYPROVIDER_H
