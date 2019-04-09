/*
  quickpaintanalyzerextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickpaintanalyzerextension.h"

#include <core/propertycontroller.h>
#include <core/paintanalyzer.h>

#include <common/objectbroker.h>

#include <QQuickPaintedItem>
#include <QPainter>

using namespace GammaRay;

QuickPaintAnalyzerExtension::QuickPaintAnalyzerExtension(PropertyController *controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".painting")
    , m_paintAnalyzer(nullptr)
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
}

QuickPaintAnalyzerExtension::~QuickPaintAnalyzerExtension() = default;

bool QuickPaintAnalyzerExtension::setQObject(QObject *object)
{
    auto item = qobject_cast<QQuickPaintedItem *>(object);
    if (!PaintAnalyzer::isAvailable() || !item)
        return false;

    m_paintAnalyzer->beginAnalyzePainting();
    m_paintAnalyzer->setBoundingRect(item->contentsBoundingRect());
    {
        QPainter painter(m_paintAnalyzer->paintDevice());
        item->paint(&painter);
    }
    m_paintAnalyzer->endAnalyzePainting();
    return true;
}
