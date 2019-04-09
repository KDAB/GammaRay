/*
  materialextension.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
