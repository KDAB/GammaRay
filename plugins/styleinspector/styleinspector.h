/*
  styleinspector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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
#include "styleinspectorwidget.h"
#include "styleinspectorinterface.h"

#include <QStyle>
#include <QWidget>

class QItemSelection;

namespace GammaRay {

class ComplexControlModel;
class ControlModel;
class PaletteModel;
class PixelMetricModel;
class PrimitiveModel;
class StandardIconModel;

class StyleInspector : public StyleInspectorInterface
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::StyleInspectorInterface)
  public:
    explicit StyleInspector(ProbeInterface *probe, QObject *parent = 0);
    virtual ~StyleInspector();

  private slots:
    void styleSelected(const QItemSelection &selection);

  private:
    PrimitiveModel *m_primitiveModel;
    ControlModel *m_controlModel;
    ComplexControlModel *m_complexControlModel;
    PixelMetricModel *m_pixelMetricModel;
    StandardIconModel *m_standardIconModel;
    PaletteModel *m_standardPaletteModel;
};

class StyleInspectorFactory : public QObject, public StandardToolFactory2<QStyle, StyleInspector, StyleInspectorWidget>
{
  Q_OBJECT
  Q_INTERFACES(GammaRay::ToolFactory GammaRay::ToolUiFactory)
  Q_PLUGIN_METADATA(IID "com.kdab.gammaray.StyleInspector")

  public:
    explicit StyleInspectorFactory(QObject *parent = 0) : QObject(parent)
    {
    }

    virtual QString name() const
    {
      return tr("Style");
    }
};

}

#endif // GAMMARAY_STYLEINSPECTOR_H
