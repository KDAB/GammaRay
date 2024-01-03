/*
  styleinspector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
