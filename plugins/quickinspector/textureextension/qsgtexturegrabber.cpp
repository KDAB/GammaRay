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
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_2_0>
#include <QPainter>
#include <QQuickWindow>
#include <QSGTexture>
#include <QThread>

#include <cmath>

using namespace GammaRay;

QSGTextureGrabber* QSGTextureGrabber::s_instance = nullptr;

QSGTextureGrabber::QSGTextureGrabber(QObject* parent)
    : QObject(parent)
    , m_textureId(-1)
{
    Q_ASSERT(!s_instance);
    s_instance = this;
}

QSGTextureGrabber::~QSGTextureGrabber()
{
    s_instance = nullptr;
}

QSGTextureGrabber* QSGTextureGrabber::instance()
{
    return s_instance;
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

    auto context = QOpenGLContext::currentContext();
    // TODO this check is only correct with the threaded render loop, but might fail with multiple windows and the basic loop
    if (m_pendingTexture && QThread::currentThread() == m_pendingTexture->thread()) {
        const auto img = grabTexture(context, m_pendingTexture->textureId());
        if (!img.isNull())
            emit textureGrabbed(m_pendingTexture, img);
        resetRequest();
    }

    // See below, this is missing a context check here. So we rely purely on the
    // safety and plausibility checks in grabTexture.
    if (m_textureId >= 0) {
        const auto img = grabTexture(context, m_textureId);
        if (!img.isNull())
            emit textureGrabbed(nullptr, img); // TODO alternative signal
        resetRequest();
    }
}

QImage QSGTextureGrabber::grabTexture(QOpenGLContext* context, int textureId) const
{
    if (context->isOpenGLES()) {
        auto glFuncs = context->functions();
        Q_ASSERT(glFuncs);

        int prev_fbo = -1;
        unsigned int fbo = -1;
        glFuncs->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);
        glFuncs->glGenFramebuffers(1, &fbo);
        glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFuncs->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
        QImage img(m_textureSize.width(), m_textureSize.height(), QImage::Format_RGBA8888_Premultiplied);
        glFuncs->glReadPixels(0, 0, m_textureSize.width(), m_textureSize.height(), GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
        glFuncs->glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
        glFuncs->glDeleteFramebuffers(1, &fbo);
        return img;
    } else {
        auto glFuncs = context->versionFunctions<QOpenGLFunctions_2_0>();
        if (!glFuncs) {
            qWarning() << "unable to obtain OpenGL2 functions, too old GL version?";
            return QImage();
        }

        QImage img(m_textureSize.width(), m_textureSize.height(), QImage::Format_ARGB32_Premultiplied);
        glFuncs->glBindTexture(GL_TEXTURE_2D, textureId);
        glFuncs->glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, img.bits());
        return img;
    }

    return QImage();
}

void QSGTextureGrabber::requestGrab(QSGTexture* tex)
{
    QMutexLocker lock(&m_mutex);
    m_pendingTexture = tex;
    const int w = std::ceil(m_pendingTexture->textureSize().width() / m_pendingTexture->normalizedTextureSubRect().width());
    const int h = std::ceil(m_pendingTexture->textureSize().height() / m_pendingTexture->normalizedTextureSubRect().height());
    m_textureSize = QSize(w, h);
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
