/*
  qt3dpaintedtextureanalyzerextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "qt3dpaintedtextureanalyzerextension.h"

#include <core/propertycontroller.h>
#include <core/paintanalyzer.h>

#include <common/objectbroker.h>

#include <Qt3DRender/QPaintedTextureImage>

#include <QPainter>

using namespace GammaRay;

class QPaintedTextureImagePrivacyViolater : public Qt3DRender::QPaintedTextureImage
{
public:
    using Qt3DRender::QPaintedTextureImage::paint;
};

Qt3DPaintedTextureAnalyzerExtension::Qt3DPaintedTextureAnalyzerExtension(PropertyController *controller)
    : PropertyControllerExtension(controller->objectBaseName() + ".painting")
    , m_paintAnalyzer(nullptr)
{
    // check if the paint analyzer already exists before creating it,
    // as we share the UI with the other plugins.
    const QString aName = controller->objectBaseName() + QStringLiteral(".painting.analyzer");
    if (ObjectBroker::hasObject(aName)) {
        m_paintAnalyzer = qobject_cast<PaintAnalyzer *>(ObjectBroker::object<PaintAnalyzerInterface *>(aName));
    } else {
        m_paintAnalyzer = new PaintAnalyzer(aName, controller);
    }
}

Qt3DPaintedTextureAnalyzerExtension::~Qt3DPaintedTextureAnalyzerExtension()
{
}

bool Qt3DPaintedTextureAnalyzerExtension::setQObject(QObject *object)
{
    if (!PaintAnalyzer::isAvailable())
        return false;

    if (auto tex = qobject_cast<Qt3DRender::QAbstractTexture *>(object)) {
        if (tex->textureImages().size() == 1)
            return setQObject(tex->textureImages().at(0));
    }

    auto tex = qobject_cast<Qt3DRender::QPaintedTextureImage *>(object);
    if (!tex || tex->size() == QSize(0, 0))
        return false;

    m_paintAnalyzer->beginAnalyzePainting();
    m_paintAnalyzer->setBoundingRect(QRect(QPoint(0, 0), tex->size()));
    {
        QPainter p(m_paintAnalyzer->paintDevice());
        reinterpret_cast<QPaintedTextureImagePrivacyViolater *>(tex)->paint(&p);
    }
    m_paintAnalyzer->endAnalyzePainting();
    return true;
}
