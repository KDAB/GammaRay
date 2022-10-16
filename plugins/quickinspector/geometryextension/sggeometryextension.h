/*
  sggeometryextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_SGGEOMETRYEXTENSION_H
#define GAMMARAY_QUICKINSPECTOR_SGGEOMETRYEXTENSION_H

#include <core/propertycontrollerextension.h>

QT_BEGIN_NAMESPACE
class QSGGeometryNode;
QT_END_NAMESPACE

namespace GammaRay {
class PropertyController;
class SGVertexModel;
class SGAdjacencyModel;

class SGGeometryExtension : public PropertyControllerExtension
{
public:
    explicit SGGeometryExtension(PropertyController *controller);
    ~SGGeometryExtension();

    bool setObject(void *object, const QString &typeName) override;

private:
    QSGGeometryNode *m_node;
    SGVertexModel *m_vertexModel;
    SGAdjacencyModel *m_adjacencyModel;
};
}

#endif // SGGEOMETRYEXTENSION_H
