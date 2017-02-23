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

class QQmlBinding;

namespace GammaRay {

class QmlBindingNode {
public:

    QmlBindingNode(QQmlBinding *binding, QmlBindingNode *parent = 0);
    QmlBindingNode(QObject *object, int propertyIndex, QmlBindingNode *parent = 0);

    QmlBindingNode *parent() const;
    QObject *object() const;

    /**
     * The qml id of the target object of this binding. No GammaRay objectId.
     */
    const QString &id() const;
    int propertyIndex() const;
    QMetaProperty property() const;
    QQmlBinding *binding() const;
    bool isBinding() const;
    bool isActive() const;
    bool isBindingLoop() const;
    const QString &expression() const;
    const SourceLocation &sourceLocation() const;
    const std::vector<std::unique_ptr<QmlBindingNode>> &dependencies() const;
    const QVariant &value() const;
    uint depth() const;

    /**
     * Refreshes
     * * the property value
     * * the check for binding loops
     * recursively, i.e. also for all its dependencies.
     */
    void refresh();

private:
    static QQmlBinding *bindingForProperty(QObject *obj, int propertyIndex);

    void checkForLoops();
    void findDependencies();
    void fetchBindingCode();
    void fetchPropertyCode();

    QmlBindingNode *m_parent;
    QObject *m_object;
    QString m_id;
    int m_propertyIndex;
    QQmlBinding *m_binding = 0;
    QVariant m_value;
    bool m_isActive = true;
    bool m_isBindingLoop = false;
    QString m_expression;
    SourceLocation m_sourceLocation;
    std::vector<std::unique_ptr<QmlBindingNode>> m_dependencies = {};

};

QDebug &operator<< (QDebug dbg, const QmlBindingNode &node);

}

#endif // GAMMARAY_QMLBINDINGNODE_H
