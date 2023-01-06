/*
  widgetpaintanalyzerextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "widgetpaintanalyzerextension.h"

#include <core/propertycontroller.h>
#include <core/paintanalyzer.h>

#include <common/objectbroker.h>

#include <QWidget>

using namespace GammaRay;

WidgetPaintAnalyzerExtension::WidgetPaintAnalyzerExtension(PropertyController *controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".painting")
    , m_paintAnalyzer(nullptr)
    , m_widget(nullptr)
{
    // check if the paint analyzer already exists before creating it,
    // as we share the UI with the other plugins.
    const QString aName = controller->objectBaseName() + QStringLiteral(".painting.analyzer");
    if (ObjectBroker::hasObject(aName)) {
        m_paintAnalyzer =
            qobject_cast<PaintAnalyzer *>(ObjectBroker::object<PaintAnalyzerInterface *>(aName));
    } else {
        m_paintAnalyzer = new PaintAnalyzer(aName, controller);
    }

    QObject::connect(m_paintAnalyzer, &PaintAnalyzer::requestUpdate, [this]() { analyze(); });
}

WidgetPaintAnalyzerExtension::~WidgetPaintAnalyzerExtension() = default;

bool WidgetPaintAnalyzerExtension::setQObject(QObject *object)
{
    m_widget = qobject_cast<QWidget *>(object);
    if (!PaintAnalyzer::isAvailable() || !m_widget)
        return false;

    m_paintAnalyzer->reset();
    return true;
}

void WidgetPaintAnalyzerExtension::analyze()
{
    if (!m_widget)
        return;
    m_paintAnalyzer->beginAnalyzePainting();
    m_paintAnalyzer->setBoundingRect(m_widget->rect());
    m_widget->render(m_paintAnalyzer->paintDevice(), QPoint(), QRegion(), {});
    m_paintAnalyzer->endAnalyzePainting();
}
