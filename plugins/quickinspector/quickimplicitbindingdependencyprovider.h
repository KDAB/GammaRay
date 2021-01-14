/*
  quickimplicitbindingdependencyprovider.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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
    std::vector<std::unique_ptr<BindingNode>> findBindingsFor(QObject * obj) const override;
    std::vector<std::unique_ptr<BindingNode>> findDependenciesFor(GammaRay::BindingNode * binding) const override;
    bool canProvideBindingsFor(QObject *object) const override;

private:
    std::unique_ptr<BindingNode> createBindingNode(QObject *obj, const char *propertyName, BindingNode *parent = nullptr) const;
    void anchorBindings(std::vector<std::unique_ptr<BindingNode>> &dependencies, QQuickAnchors *anchors, int propertyIndex, BindingNode *parent = nullptr) const;
    template<class Func> void childrenRectDependencies(QQuickItem *item, Func addDependency) const;
    template<class Func> void positionerDependencies(QQuickItem *item, Func addDependency) const;
    template<class Func> void implicitSizeDependencies(QQuickItem *item, Func addDependency) const;
    template<class Func> void anchoringDependencies(QQuickItem *item, Func addDependency) const;
};

}

#endif // GAMMARAY_QUICKIMPLICITBINDINGDEPENDENCYPROVIDER_H
