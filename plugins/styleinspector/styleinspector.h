/*
  styleinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#ifndef GAMMARAY_STYLEINSPECTOR_STYLEINSPECTOR_H
#define GAMMARAY_STYLEINSPECTOR_STYLEINSPECTOR_H

#include <core/toolfactory.h>
#include "styleinspectorinterface.h"

#include <QStyle>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
class ComplexControlModel;
class ControlModel;
class PaletteModel;
class PixelMetricModel;
class PrimitiveModel;
class StandardIconModel;
class StyleHintModel;

class StyleInspector : public StyleInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::StyleInspectorInterface)
public:
    explicit StyleInspector(Probe *probe, QObject *parent = nullptr);
    ~StyleInspector() override;

private slots:
    void styleSelected(const QItemSelection &selection);

private:
    PrimitiveModel *m_primitiveModel;
    ControlModel *m_controlModel;
    ComplexControlModel *m_complexControlModel;
    PixelMetricModel *m_pixelMetricModel;
    StandardIconModel *m_standardIconModel;
    PaletteModel *m_standardPaletteModel;
    StyleHintModel *m_styleHintModel;
};

class StyleInspectorFactory : public QObject, public StandardToolFactory<QStyle, StyleInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_styleinspector.json")

public:
    explicit StyleInspectorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_STYLEINSPECTOR_H
