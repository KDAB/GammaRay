/*
  bindingnode.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    BindingNode(QObject *object, int propertyIndex, BindingNode *parent = nullptr);

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
