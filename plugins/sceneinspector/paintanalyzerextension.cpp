/*
  paintanalyzerextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "paintanalyzerextension.h"

#include <core/propertycontroller.h>
#include <core/paintanalyzer.h>
#include <core/metaobjectrepository.h>
#include <core/metaobject.h>

#include <common/objectbroker.h>

#include <QDebug>
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>
#include <QPainter>

using namespace GammaRay;

PaintAnalyzerExtension::PaintAnalyzerExtension(PropertyController *controller)
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

PaintAnalyzerExtension::~PaintAnalyzerExtension() = default;

bool PaintAnalyzerExtension::setQObject(QObject *object)
{
    if (!PaintAnalyzer::isAvailable())
        return false;

    if (auto qgvObj = qobject_cast<QGraphicsObject *>(object))
        return analyzePainting(qgvObj);

    return false;
}

bool PaintAnalyzerExtension::setObject(void *object, const QString &typeName)
{
    if (!PaintAnalyzer::isAvailable())
        return false;

    const auto mo = MetaObjectRepository::instance()->metaObject(typeName);
    if (!mo)
        return false;
    if (const auto item = mo->castTo(object, QStringLiteral("QGraphicsItem")))
        return analyzePainting(static_cast<QGraphicsItem *>(item));
    return false;
}

bool PaintAnalyzerExtension::analyzePainting(QGraphicsItem *item)
{
    if (item->flags() & QGraphicsItem::ItemHasNoContents)
        return false;

    m_paintAnalyzer->beginAnalyzePainting();
    m_paintAnalyzer->setBoundingRect(item->boundingRect());

    QStyleOptionGraphicsItem option;
    option.state = QStyle::State_None;
    option.rect = item->boundingRect().toRect();

    /* Commented because no longer available in Qt6
     * The alternative 'levelOfDetailFromTransform'
     * is available, but docs say its value will be 1 by
     * default anyways
     */
    // option.levelOfDetail = 1;

    option.exposedRect = item->boundingRect();

    option.styleObject = item->toGraphicsObject();
    if (!option.styleObject)
        option.styleObject = item->scene();

    if (item->isSelected())
        option.state |= QStyle::State_Selected;
    if (item->isEnabled())
        option.state |= QStyle::State_Enabled;
    if (item->hasFocus())
        option.state |= QStyle::State_HasFocus;

    {
        QPainter p(m_paintAnalyzer->paintDevice());
        item->paint(&p, &option);
    }
    m_paintAnalyzer->endAnalyzePainting();
    return true;
}
