/*
  qmlbindingnode.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_QMLBINDINGNODE_H
#define GAMMARAY_QMLBINDINGNODE_H

#include <QAbstractTableModel>
#include <QVector>
#include <memory>
#include <vector>

#include <common/sourcelocation.h>
#include <QQmlProperty>

class QQmlAbstractBinding;
class QQmlBinding;

namespace GammaRay {

class QmlBindingExtension;

struct BindingNode
{
public:
    BindingNode (QObject *object, int propertyIndex, BindingNode *parent = Q_NULLPTR);
    BindingNode(const BindingNode &other);

    QMetaProperty property() const;
    uint depth() const;

    void refreshValue();
    void checkForLoops();

    BindingNode *parent;
    QObject *object;
    int propertyIndex;
    /**
     * An internal id that can be used to compare binding nodes. Do not use it to cast it back to a pointer.
     */
    qintptr id;
    QString canonicalName;
    QVariant value;
    bool isActive = true;
    bool isBindingLoop = false;
    QString expression;
    SourceLocation sourceLocation;
    std::vector<std::unique_ptr<BindingNode>> dependencies = {};

//TODO: private:
    BindingNode(QObject *object, int propertyIndex, qintptr id); // Only for testing

    friend class BindingInspectorTest;
};

class AbstractBindingProvider
{
public:
    virtual ~AbstractBindingProvider();
    virtual std::vector<std::unique_ptr<BindingNode>> findDependenciesFor(BindingNode *binding) = 0;
    virtual std::vector<std::unique_ptr<BindingNode>> findBindingsFor(QObject *obj) = 0;
    virtual bool canProvideBindingsFor (QObject *object) = 0;
};

class QmlBindingProvider : public AbstractBindingProvider
{
public:
    std::vector<std::unique_ptr<BindingNode>> findBindingsFor(QObject * obj) override;
    std::vector<std::unique_ptr<BindingNode>> findDependenciesFor(GammaRay::BindingNode * binding) override;

private:
    std::unique_ptr<BindingNode> bindingNodeFromQmlProperty(QQmlProperty property, BindingNode *parent);
    BindingNode *bindingNodeFromBinding(QQmlAbstractBinding *binding);
    void fetchSourceLocationFor(BindingNode *node, QQmlBinding *binding);
    QQmlAbstractBinding *bindingForProperty(QObject *obj, int propertyIndex);
    bool canProvideBindingsFor(QObject * object) override;
};

class QuickImplicitBindingDependencyProvider : public AbstractBindingProvider
{
public:
    std::vector<std::unique_ptr<BindingNode>> findBindingsFor(QObject * obj) override;
    std::vector<std::unique_ptr<BindingNode>> findDependenciesFor(GammaRay::BindingNode * binding) override;
    bool canProvideBindingsFor(QObject *object) override;

private:
    std::unique_ptr<BindingNode> createBindingNode(QObject *obj, const char *propertyName) const;
};

}

#endif // GAMMARAY_QMLBINDINGNODE_H

