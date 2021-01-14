/*
  qsgtexturegrabber.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
#include <QOpenGLExtraFunctions>
#endif

#ifndef GL_TEXTURE_WIDTH
#define GL_TEXTURE_WIDTH 0x1000
#endif
#ifndef GL_TEXTURE_HEIGHT
#define GL_TEXTURE_HEIGHT 0x1001
#endif
#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

#include <cmath>

using namespace GammaRay;

QSGTextureGrabber *QSGTextureGrabber::s_instance = nullptr;

QSGTextureGrabber::QSGTextureGrabber(QObject *parent)
    : QObject(parent)
    , m_grabData(nullptr)
    , m_textureId(-1)
{
    Q_ASSERT(!s_instance);
    s_instance = this;
}

QSGTextureGrabber::~QSGTextureGrabber()
{
    s_instance = nullptr;
}

QSGTextureGrabber *QSGTextureGrabber::instance()
{
    return s_instance;
}

void QSGTextureGrabber::objectCreated(QObject *obj)
{
    if (auto window = qobject_cast<QQuickWindow*>(obj))
        addQuickWindow(window);
}

void QSGTextureGrabber::addQuickWindow(QQuickWindow *window)
{
    connect(window, &QQuickWindow::afterRendering, this, [this, window]() {
        windowAfterRendering(window);
    }, Qt::DirectConnection);
    m_windows.emplace_back(window);
}

void QSGTextureGrabber::windowAfterRendering(QQuickWindow *window)
{
    QMutexLocker lock(&m_mutex);
    if (!m_pendingTexture && m_textureId <= 0) {
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    if (window->rendererInterface()->graphicsApi() != QSGRendererInterface::OpenGL) {
        return;
    }
#else
    Q_UNUSED(window);
#endif

    auto context = QOpenGLContext::currentContext();
    // This check is only correct with the threaded render loop, for the basic one this
    // can fail with a non-shared context (if that is even a plausible scenario)
    // We can't detect this, so we rely on our safety checks in grabTexture and accept
    // a minimal chance of showing texture content from the wrong context.
    if (m_pendingTexture && QThread::currentThread() == m_pendingTexture->thread()) {
        if (m_pendingTexture->textureId() > 0) {
            const auto img = grabTexture(context, m_pendingTexture->textureId());
            if (!img.isNull()) {
                emit textureGrabbed(m_pendingTexture, img);
            }
        }
        resetRequest();
    }

    // See below, this is missing a context check here. So we rely purely on the
    // safety and plausibility checks in grabTexture.
    if (m_textureId > 0) {
        const auto img = grabTexture(context, m_textureId);
        if (!img.isNull()) {
            emit textureGrabbed(m_grabData, img);
        }
        resetRequest();
    }

    window->resetOpenGLState();
}

QImage QSGTextureGrabber::grabTexture(QOpenGLContext *context, int textureId) const
{
    if (context->isOpenGLES()) {
        auto glFuncs = context->functions();
        Q_ASSERT(glFuncs);

        // Reset the error flag, it can be a pending error that will conflict with our later check
        glFuncs->glGetError();

        glFuncs->glBindTexture(GL_TEXTURE_2D, textureId);
        if (const auto err = glFuncs->glGetError()) {
            qWarning() << "Unable to bind texture for grabbing:" << err;
            return QImage();
        }

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
        // check if the size matches our expectations (requires ES3.1, so we might have to skip this
        auto glExtraFuncs = context->extraFunctions();
        if (glExtraFuncs) {
            int w = 0, h = 0;
            glExtraFuncs->glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
            glExtraFuncs->glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
            if (m_textureSize.width() != w || m_textureSize.height() != h) {
                qWarning() << "OpenGL reported texture sizes doesn't match our assumption, aborting texture grab!" << m_textureSize << w << h;
                return QImage();
            }
        } else {
            qDebug() << "Can't validate texture size (OpenGL ES < 3.1), things might go wrong in a multi-context scenario...";
        }
#endif

        // bind texture to an FBO, and read that, direct texture reading is not supported with OpenGL ES
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
#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_2)
        auto glFuncs = context->versionFunctions<QOpenGLFunctions_2_0>();
        if (!glFuncs) {
            qWarning() << "unable to obtain OpenGL2 functions, too old GL version?";
            return QImage();
        }

        // Reset the error flag, it can be a pending error that will conflict with our later check
        glFuncs->glGetError();

        glFuncs->glBindTexture(GL_TEXTURE_2D, textureId);
        if (const auto err = glFuncs->glGetError()) {
            qWarning() << "Unable to bind texture for grabbing:" << err;
            return QImage();
        }

        // check if the size matches our expectations
        int w = 0, h = 0;
        glFuncs->glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
        glFuncs->glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
        if (m_textureSize.width() != w || m_textureSize.height() != h) {
            qWarning() << "OpenGL reported texture sizes doesn't match our assumption, aborting texture grab!" << m_textureSize << w << h;
            return QImage();
        }

        // actually read the texture content
        QImage img(m_textureSize.width(), m_textureSize.height(), QImage::Format_ARGB32_Premultiplied);
        glFuncs->glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, img.bits());
        return img;
#endif
    }
    return QImage();
}

void QSGTextureGrabber::requestGrab(QSGTexture *tex)
{
    QMutexLocker lock(&m_mutex);
    m_pendingTexture = tex;
    const int w = std::ceil(m_pendingTexture->textureSize().width() / m_pendingTexture->normalizedTextureSubRect().width());
    const int h = std::ceil(m_pendingTexture->textureSize().height() / m_pendingTexture->normalizedTextureSubRect().height());
    m_textureSize = QSize(w, h);
    triggerUpdate();
}

void QSGTextureGrabber::requestGrab(int textureId, const QSize &texSize, void *data)
{
    if (textureId < 0 || !texSize.isValid())
        return;

    QMutexLocker lock(&m_mutex);
    // ### we are missing some context here to check if we are in the right GL context/render thread for grabbing
    // best idea so far: use the QQItem from the texture extension, making this unavailable from the QSG view though
    m_textureId = textureId;
    m_textureSize = texSize;
    m_grabData = data;
    triggerUpdate();
}

void QSGTextureGrabber::triggerUpdate()
{
    for (auto it = m_windows.begin(); it != m_windows.end();) {
        if (*it) {
            (*it)->update();
            ++it;
        } else {
            it = m_windows.erase(it);
        }
    }
}

void QSGTextureGrabber::resetRequest()
{
    m_pendingTexture = nullptr;
    m_textureId = -1;
}
