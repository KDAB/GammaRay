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

namespace GammaRay {

class QmlBindingExtension;

class BindingNode
{
public:
    BindingNode (QObject *object, int propertyIndex, BindingNode *parent = Q_NULLPTR);

    void refreshValue();


    /**
     * A name describing the binding (e.g. "r1.width")
     */
    const QString &name() const;
    /**
     * An internal id that can be used to compare binding nodes. Do not use it to cast it back to a pointer.
     */
    qintptr id() const;
    BindingNode *parent() const;
    void setParent( BindingNode *parent);
    QObject *object() const;
    int propertyIndex() const;
    QMetaProperty property() const;
    bool isActive() const;
    bool isBindingLoop() const;
    const QString &expression() const;
    const SourceLocation &sourceLocation() const;
    std::vector<std::unique_ptr<BindingNode>> &dependencies();
    const std::vector<std::unique_ptr<BindingNode>> &dependencies() const;
    void addDependency(std::unique_ptr<BindingNode> dependency);
    void clearDependencies();
    const QVariant &value() const;
    uint depth() const;

    bool operator<(const BindingNode &other) const;
    bool operator>(const BindingNode &other) const;


    void setName(const QString &name);
    void setId(qintptr id);
    void setActive(bool active);
    void setIsBindingLoop(bool isLoop);
    void setExpression(const QString &expression);
    void setSourceLocation(const SourceLocation &location);

    void checkForLoops();

    std::unique_ptr<BindingNode> refresh();

private:
    BindingNode *m_parent;
    qintptr m_id;
    QObject *m_object;
    QString m_name;
    int m_propertyIndex;
    QVariant m_value;
    bool m_isActive = true;
    bool m_isBindingLoop = false;
    QString m_expression;
    SourceLocation m_sourceLocation;
    std::vector<std::unique_ptr<BindingNode>> m_dependencies = {};
};

class AbstractBindingProvider
{
public:
    struct Dependency {
        QObject *object;
        int propertyIndex;
    };
    virtual ~AbstractBindingProvider();
    virtual std::vector<Dependency> findDependenciesFor(BindingNode *binding) = 0;
    virtual std::vector<std::unique_ptr<BindingNode>> findBindingsFor(QObject *obj) = 0;
    virtual bool canProvideBindingsFor (QObject *object) = 0;
};

class QmlBindingProvider : public AbstractBindingProvider
{
public:
    std::vector<std::unique_ptr<BindingNode>> findBindingsFor(QObject * obj) override;
    std::vector<Dependency> findDependenciesFor(GammaRay::BindingNode * binding) override;

private:
    std::unique_ptr<BindingNode> bindingNodeFromQmlProperty(QQmlProperty property, BindingNode *parent);
    BindingNode *bindingNodeFromBinding(QQmlAbstractBinding *binding);
    void fetchSourceLocationFor(BindingNode *node, QQmlAbstractBinding *binding);
    QQmlAbstractBinding *bindingForProperty(QObject *obj, int propertyIndex);
    bool canProvideBindingsFor(QObject * object) override;
};

class QuickImplicitBindingDependencyProvider : public AbstractBindingProvider
{
public:
    std::vector<std::unique_ptr<BindingNode>> findBindingsFor(QObject * obj) override;
    std::vector<Dependency> findDependenciesFor(GammaRay::BindingNode * binding) override;
    bool canProvideBindingsFor(QObject *object) override;
};

}

#endif // GAMMARAY_QMLBINDINGNODE_H

