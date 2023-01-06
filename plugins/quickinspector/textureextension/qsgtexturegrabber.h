/*
  qsgtexturegrabber.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QSGTEXTUREGRABBER_H
#define GAMMARAY_QSGTEXTUREGRABBER_H

#include <QMutex>
#include <QPointer>
#include <QObject>
#include <QSGTexture>

#include <vector>

QT_BEGIN_NAMESPACE
class QImage;
class QOpenGLContext;
class QQuickWindow;
QT_END_NAMESPACE

namespace GammaRay {

class QSGTextureGrabber : public QObject
{
    Q_OBJECT
public:
    explicit QSGTextureGrabber(QObject *parent = nullptr);
    ~QSGTextureGrabber() override;

    static QSGTextureGrabber *instance();

public slots:
    void objectCreated(QObject *obj);
    void requestGrab(QSGTexture *tex);
    void requestGrab(int textureId, const QSize &texSize, void *data);

signals:
    void textureGrabbed(QSGTexture *tex, const QImage &img);
    void textureGrabbed(void *data, const QImage &img);

private:
    void addQuickWindow(QQuickWindow *window);
    void windowAfterRendering(QQuickWindow *window);
    QImage grabTexture(QOpenGLContext *context, int textureId) const;

    void triggerUpdate();
    void resetRequest();

    static QSGTextureGrabber *s_instance;

    QMutex m_mutex;
    QPointer<QSGTexture> m_pendingTexture;
    std::vector<QPointer<QQuickWindow>> m_windows;

    void *m_grabData;
    int m_textureId;
    QSize m_textureSize;
};
}

#endif // GAMMARAY_QSGTEXTUREGRABBER_H
