/*
  sggeometryextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
