/*
  bindingnode.h

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

#include "gammaray_core_export.h"

#include <common/sourcelocation.h>

class QQmlAbstractBinding;
class QQmlBinding;

namespace GammaRay {

class QmlBindingExtension;

struct GAMMARAY_CORE_EXPORT BindingNode
{
public:
    BindingNode (QObject *object, int propertyIndex, BindingNode *parent = Q_NULLPTR);
    BindingNode(const BindingNode &other);
    BindingNode(BindingNode &&other);

    QMetaProperty property() const;
    uint depth() const;

    void refreshValue();
    void checkForLoops();

    void setParent(BindingNode *newParent);

    QVariant cachedValue() const;
    QVariant readValue() const;

    BindingNode *parent() const;
    QObject *object() const;
    int propertyIndex() const;
    qintptr id() const;
    const QString &canonicalName() const;
    bool isActive() const;
    bool isBindingLoop() const;
    const QString &expression() const;
    SourceLocation sourceLocation() const;
    std::vector<std::unique_ptr<BindingNode>> &dependencies();
    const std::vector<std::unique_ptr<BindingNode>> &dependencies() const;

    void setId(qintptr id);
    void setSourceLocation(SourceLocation location);
    void setCanonicalName(const QString &name);

private:
    BindingNode *m_parent;
    QObject *m_object;
    int m_propertyIndex;
    /**
     * An internal id that can be used to compare binding nodes. Do not use it to cast it back to a pointer.
     */
    qintptr m_id;
    QString m_canonicalName;
    QVariant m_value;
    bool m_isActive = true;
    bool m_isBindingLoop = false;
    QString m_expression;
    SourceLocation m_sourceLocation;
    std::vector<std::unique_ptr<BindingNode>> m_dependencies = {};

    friend class MockBindingProvider;
};

}

#endif // GAMMARAY_QMLBINDINGNODE_H

