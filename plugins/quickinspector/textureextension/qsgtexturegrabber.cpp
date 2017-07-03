/*
  qsgtexturegrabber.cpp

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

#include "qsgtexturegrabber.h"

#include <QDebug>
#include <QImage>
#include <QOpenGLContext>
#include <QOpenGLFunctions_2_0>
#include <QPainter>
#include <QQuickWindow>
#include <QSGTexture>
#include <QThread>

#include <cmath>

using namespace GammaRay;

QSGTextureGrabber::QSGTextureGrabber(QObject* parent)
    : QObject(parent)
    , m_textureId(-1)
{
}

QSGTextureGrabber::~QSGTextureGrabber()
{
}

void QSGTextureGrabber::objectCreated(QObject* obj)
{
    if (auto window = qobject_cast<QQuickWindow*>(obj))
        addQuickWindow(window);
}

void QSGTextureGrabber::addQuickWindow(QQuickWindow* window)
{
    connect(window, &QQuickWindow::afterRendering, this, [this, window]() {
        windowAfterRendering(window);
    }, Qt::DirectConnection);
}

void QSGTextureGrabber::windowAfterRendering(QQuickWindow* window)
{
    QMutexLocker lock(&m_mutex);
    if (!m_pendingTexture && m_textureId < 0)
        return;

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    if (window->rendererInterface()->graphicsApi() != QSGRendererInterface::OpenGL)
        return;
#endif

    // TODO this check is only correct with the threaded render loop, but might fail with multiple windows and the basic loop
    if (m_pendingTexture && QThread::currentThread() == m_pendingTexture->thread()) {
        qWarning() << "found the context!" << m_pendingTexture->textureSize() << m_pendingTexture->isAtlasTexture() << m_pendingTexture->normalizedTextureSubRect();

        const int w = std::ceil(m_pendingTexture->textureSize().width() / m_pendingTexture->normalizedTextureSubRect().width());
        const int h = std::ceil(m_pendingTexture->textureSize().height() / m_pendingTexture->normalizedTextureSubRect().height());

        auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_2_0>();
        if (!glFuncs) {
            // TODO
            qDebug() << "probably ES2, find a way to grab the content there";
            return;
        }
        QImage img(w, h, QImage::Format_ARGB32_Premultiplied);
        m_pendingTexture->bind();
        // TODO GL_BGRA is not always correct I think!
        glFuncs->glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, img.bits());

        emit textureGrabbed(m_pendingTexture, img);
        resetRequest();
    }

    // See below, this is missing a context check here. So we rely purely on the
    // safety and plausibility checks in grabTexture.
    if (m_textureId >= 0) {
        auto glFuncs = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_2_0>();
        if (!glFuncs) {
            // TODO
            qDebug() << "probably ES2, find a way to grab the content there";
            return;
        }
        QImage img(m_textureSize.width(), m_textureSize.height(), QImage::Format_ARGB32_Premultiplied);
        glFuncs->glBindTexture(GL_TEXTURE_2D, m_textureId);
        // TODO GL_BGRA is not always correct I think!
        glFuncs->glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, img.bits());

        emit textureGrabbed(nullptr, img); // TODO alternative signal
        resetRequest();
    }
}

void QSGTextureGrabber::requestGrab(QSGTexture* tex)
{
    QMutexLocker lock(&m_mutex);
    m_pendingTexture = tex;
}

void QSGTextureGrabber::requestGrab(int textureId, const QSize& texSize)
{
    if (textureId < 0 || !texSize.isValid())
        return;

    QMutexLocker lock(&m_mutex);
    // ### we are missing some context here to check if we are in the right GL context/render thread for grabbing
    // best idea so far: use the QQItem from the texture extension, making this unavailable from the QSG view though
    m_textureId = textureId;
    m_textureSize = texSize;
}

void QSGTextureGrabber::resetRequest()
{
    m_pendingTexture = nullptr;
    m_textureId = -1;
}
