/*
  textureextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "textureextension.h"

#include <core/propertycontroller.h>
#include <core/util.h>
#include <core/remoteviewserver.h>
#include <common/objectbroker.h>
#include <common/remoteviewframe.h>

#include <QDebug>
#include <QImage>
#include <QQuickItem>
#include <QSGGeometryNode>
#include <QSGOpaqueTextureMaterial>
#include <QSGTexture>

#include <private/qquickitem_p.h>
#include <private/qsgdistancefieldglyphnode_p_p.h>

using namespace GammaRay;

TextureExtension::TextureExtension(PropertyController *controller)
    : QObject(controller)
    , PropertyControllerExtension(controller->objectBaseName() + ".texture")
    , m_currentTexture(nullptr)
    , m_qsgGrabber(nullptr)
    , m_remoteView(new RemoteViewServer(controller->objectBaseName() + ".texture.remoteView", controller))
{
     m_qsgGrabber = ObjectBroker::object<QObject*>(QStringLiteral("com.kdab.GammaRay.QSGTextureGrabber"));
     Q_ASSERT(m_qsgGrabber);
     connect(m_qsgGrabber, SIGNAL(textureGrabbed(QSGTexture*,QImage)), this, SLOT(textureGrabbed(QSGTexture*,QImage)));
}

TextureExtension::~TextureExtension()
{
}

static QSGGeometryNode *findGeometryNode(QSGNode *node)
{
    while(node) {
        if (node->type() == QSGNode::GeometryNodeType)
            return static_cast<QSGGeometryNode*>(node);
        if (node->childCount() == 0)
            return nullptr;
        if (node->childCount() > 1 && node->firstChild()->type() != QSGNode::GeometryNodeType)
            return nullptr;
        node = node->firstChild();
    }

    return nullptr;
}

bool TextureExtension::setQObject(QObject* obj)
{
    m_currentTexture = nullptr;

    if (auto qsgTexture = qobject_cast<QSGTexture*>(obj)) {
        m_remoteView->resetView();
        m_currentTexture = qsgTexture;
        QMetaObject::invokeMethod(m_qsgGrabber, "requestGrab", Q_ARG(QSGTexture*, qsgTexture));
        return true;
    }

    if (auto item = qobject_cast<QQuickItem*>(obj)) {
        if (item->metaObject() == &QQuickItem::staticMetaObject)
            return false;
        auto priv = QQuickItemPrivate::get(item);
        if (!priv->itemNodeInstance)
            return false;
        auto geometryNode = findGeometryNode(priv->itemNodeInstance);
        if (geometryNode)
            return setObject(geometryNode, "QSGGeometryNode");
    }

    return false;
}

bool TextureExtension::setObject(void* object, const QString& typeName)
{
    m_currentTexture = nullptr;

    if (typeName == QLatin1String("QSGGeometryNode")) {
        auto node = static_cast<QSGGeometryNode*>(object);
        if (Util::isNullish(node->activeMaterial()))
            return false;
        auto material = node->activeMaterial();
        if (auto mat = dynamic_cast<QSGOpaqueTextureMaterial*>(material))
            return setQObject(mat->texture());

        if (auto mat = dynamic_cast<QSGDistanceFieldTextMaterial*>(material)) {
            if (!mat->texture())
                return false;
            m_remoteView->resetView();
            QMetaObject::invokeMethod(m_qsgGrabber, "requestGrab", Q_ARG(int, mat->texture()->textureId), Q_ARG(QSize, mat->texture()->size));
            return true;
        }
    }

    return false;
}

void TextureExtension::textureGrabbed(QSGTexture* tex, const QImage& img)
{
    if (tex != m_currentTexture /* || !tex*/) // TODO reenable !tex here once we have a dedicated signal for df texture grabs
        return;

    RemoteViewFrame f;
    f.setImage(img);
    if (m_currentTexture && m_currentTexture->isAtlasTexture()) {
        QRect subRect(img.width() * m_currentTexture->normalizedTextureSubRect().x(),
                      img.height() * m_currentTexture->normalizedTextureSubRect().y(),
                      m_currentTexture->textureSize().width(), m_currentTexture->textureSize().height());
        f.setData(subRect);
    }
    m_remoteView->sendFrame(f);
}
