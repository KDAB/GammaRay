/*
  materialextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_MATERIALEXTENSION_H
#define GAMMARAY_QUICKINSPECTOR_MATERIALEXTENSION_H

#include <core/propertycontrollerextension.h>
#include "materialextensioninterface.h"

#include <memory>

QT_BEGIN_NAMESPACE
class QSGGeometryNode;
class QSGMaterialShader;
QT_END_NAMESPACE

namespace GammaRay {
class AggregatedPropertyModel;
class MaterialShaderModel;
class PropertyController;
class ObjectEnumModel;

class MaterialExtension : public MaterialExtensionInterface, public PropertyControllerExtension
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::MaterialExtensionInterface)

public:
    explicit MaterialExtension(PropertyController *controller);
    ~MaterialExtension() override;

    bool setObject(void *object, const QString &typeName) override;

public slots:
    void getShader(int row) override;

private:
    QSGGeometryNode *m_node;
    AggregatedPropertyModel *m_materialPropertyModel;
    MaterialShaderModel *m_shaderModel;
    std::unique_ptr<QSGMaterialShader> m_materialShader;
};
}

#endif // MATERIALEXTENSION_H
