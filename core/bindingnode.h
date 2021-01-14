/*
  bindingnode.h

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

#ifndef GAMMARAY_BINDINGNODE_H
#define GAMMARAY_BINDINGNODE_H

// Own
#include "gammaray_core_export.h"

#include <common/sourcelocation.h>

// Qt
#include <QVariant>

// Std
#include <memory>
#include <vector>

class MockBindingProvider;

namespace GammaRay {

class GAMMARAY_CORE_EXPORT BindingNode
{
public:
    BindingNode (QObject *object, int propertyIndex, BindingNode *parent = nullptr);

    bool operator==(const BindingNode &other) const;

    BindingNode *parent() const;
    QObject *object() const;
    int propertyIndex() const;
    QMetaProperty property() const;

    const QString &canonicalName() const;
    /**
     * This function returns true, if checkForLoops() found a loop.
     * It usually only returns true for one node in a binding loop.
     *
     * \sa isPartOfBindingLoop()
     */
    bool hasFoundBindingLoop() const;
    bool isPartOfBindingLoop() const;
    SourceLocation sourceLocation() const;
    uint depth() const;
    QVariant cachedValue() const;
    QVariant readValue() const;

    std::vector<std::unique_ptr<BindingNode>> &dependencies();
    const std::vector<std::unique_ptr<BindingNode>> &dependencies() const;

    void refreshValue();
    void checkForLoops();

    void setParent(BindingNode *newParent);
    void setSourceLocation(const SourceLocation &location);
    void setCanonicalName(const QString &name);

private:
    Q_DISABLE_COPY(BindingNode)
    BindingNode *m_parent;
    QObject *m_object;
    int m_propertyIndex;
    QString m_canonicalName;
    QVariant m_value;
    bool m_foundBindingLoop = false;
    SourceLocation m_sourceLocation;
    std::vector<std::unique_ptr<BindingNode>> m_dependencies;

    friend class ::MockBindingProvider;
};

}

#endif // GAMMARAY_BINDINGNODE_H

